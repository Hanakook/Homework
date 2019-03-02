#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/buffer_head.h>
#include <asm/switch_to.h>
#include <asm/uaccess.h>
#include <asm/dma.h>


static DEFINE_SPINLOCK(memblock_lock);//定义自旋锁
static struct request_queue * memblock_request;//申请队列
static struct gendisk *memblock_disk;//磁盘结构体
static int memblock_major;
#define BLOCKBUF_SIZE (1024*1024)    //磁盘大小
#define SECTOR_SIZE      (512)  //扇区大小
#define SECTOR_SIZE_SHIFT 9
unsigned char simp_blkdev_data[BLOCKBUF_SIZE];// 虚拟磁盘块设备的存储空间
#define SIMP_BLKDEV_DISKNAME "memblock"          //块设备名
static int memblock_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{    
       geo->heads =2;     // 2个磁头分区
       geo->cylinders = 32;   //一个磁头有32个柱面
       geo->sectors = BLOCKBUF_SIZE/(2*32*SECTOR_SIZE);  //一个柱面有多少个扇区    
       return 0;
}

static struct block_device_operations memblock_fops = {
       .owner    = THIS_MODULE,
       .getgeo   =  memblock_getgeo, //几何,保存磁盘的信息(柱头,柱面,扇区)
};

    /*申请队列处理函数*/
static void do_memblock_request (struct request_queue * q)
{
        struct request *req;// 正在处理的请求队列中的请求 
	struct bio *req_bio;// 当前请求的bio 
	struct bio_vec *bvec;// 当前请求的bio的段(segment)链表 
        char *disk_mem;      // 需要读/写的磁盘区域
	char *buffer; // 磁盘块设备的缓冲区 
	int i = 0; 
	while((req = blk_fetch_request(q)) != NULL){ // 判断当前req是否合法 
		if((blk_rq_pos(req)<<SECTOR_SIZE_SHIFT) + blk_rq_bytes(req) > BLOCKBUF_SIZE){ 
			printk(KERN_ERR SIMP_BLKDEV_DISKNAME":bad request:block=%llu, count=%u\n",(unsigned long long)blk_rq_pos(req),blk_rq_sectors(req)); 
			blk_end_request_all(req, -EIO);
			continue;
		 }																																																																		
		//获取需要操作的内存位置
        	disk_mem = simp_blkdev_data + (blk_rq_pos(req) << SECTOR_SIZE_SHIFT);
		req_bio = req->bio;
		// 获取当前请求的bio 
		switch (rq_data_dir(req)) { //判断请求的类型 
			case READ: // 遍历req请求的bio链表 
				while(req_bio != NULL){ //　for循环处理bio结构中的bio_vec结构体数组（bio_vec结构体数组代表一个完整的缓冲区） 
					for(i=0; i<req_bio->bi_vcnt; i++){ 
						bvec = &(req_bio->bi_io_vec[i]);
						buffer = kmap(bvec->bv_page) + bvec->bv_offset; 
			        		memcpy(buffer, disk_mem, bvec->bv_len);
                    				kunmap(bvec->bv_page);
                    				disk_mem += bvec->bv_len;
					} 
					req_bio = req_bio->bi_next; 
				}
			 	__blk_end_request_all(req, 0); 
				break;
			case WRITE: 
				while(req_bio != NULL){
					 for(i=0; i<req_bio->bi_vcnt; i++){ 
						bvec = &(req_bio->bi_io_vec[i]); 
						buffer = kmap(bvec->bv_page) + bvec->bv_offset; 
						memcpy(disk_mem, buffer, bvec->bv_len);
                    				kunmap(bvec->bv_page);
                    				disk_mem += bvec->bv_len;
						
					  }
					 req_bio = req_bio->bi_next;
				 }
				 __blk_end_request_all(req, 0);
				 break; 
			default:  
				break;
		}
	 }

}

    /*入口函数*/
static int memblock_init(void)
{
     /*1)使用register_blkdev()创建一个块设备*/
     memblock_major=register_blkdev(0, SIMP_BLKDEV_DISKNAME);     
     
     /*2) blk_init_queue()使用分配一个申请队列,并赋申请队列处理函数*/
     memblock_request=blk_init_queue(do_memblock_request,&memblock_lock);
    
     /*3)使用alloc_disk()分配一个gendisk结构体*/
     memblock_disk=alloc_disk(16);                        //不分区
    
     /*4)设置gendisk结构体的成员*/
     /*->4.1)设置成员参数(major、first_minor、disk_name、fops)*/           
     memblock_disk->major = memblock_major;
     memblock_disk->first_minor = 0;
     sprintf(memblock_disk->disk_name, SIMP_BLKDEV_DISKNAME);
     memblock_disk->fops = &memblock_fops;
        
     /*->4.2)设置queue成员,等于之前分配的申请队列*/
     memblock_disk->queue = memblock_request;
      
     /*->4.3)通过set_capacity()设置capacity成员,等于扇区数*/
     set_capacity(memblock_disk,BLOCKBUF_SIZE/SECTOR_SIZE);
   
   
 
     /*6)使用add_disk()注册gendisk结构体*/
     add_disk(memblock_disk); 

     //test
     //printk("hello\n");  
     return  0;
}
static void memblock_exit(void)
{        
      /*1)使用put_disk()和del_gendisk()来注销,释放gendisk结构体*/
      put_disk(memblock_disk);
      del_gendisk(memblock_disk);

 
      /*2)使用blk_cleanup_queue()清除内存中的申请队列    */
      blk_cleanup_queue(memblock_request);
      
      /*3)使用unregister_blkdev()卸载块设备               */
      unregister_blkdev(memblock_major,SIMP_BLKDEV_DISKNAME);
      //printk("goodbye\n"); 
}



module_init(memblock_init);
module_exit(memblock_exit);
MODULE_LICENSE("GPL");


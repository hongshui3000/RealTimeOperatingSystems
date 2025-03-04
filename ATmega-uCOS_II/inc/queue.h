#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifndef NOT_OK
#define NOT_OK              		0xFF  	/* 参数错误                           */
#endif

#define QUEUE_FULL          		8     	/* 队列满                             */
#define QUEUE_EMPTY         		4     	/* 无数据                             */
#define QUEUE_OK            		1     	/* 操作成功                           */

#define Q_WRITE_MODE        		1     	/* 操作成功                           */
#define Q_WRITE_FRONT_MODE  		2     	/* 操作成功                           */

#define EN_QUEUE_WRITE            	1     	/* 禁止(0)或允许(1)FIFO发送数据       */
#define EN_QUEUE_WRITE_FRONT      	1     	/* 禁止(0)或允许(1)LIFO发送数据       */
#define EN_QUEUE_NDATA            	1     	/* 禁止(0)或允许(1)取得队列数据数目   */
#define EN_QUEUE_SIZE             	1     	/* 禁止(0)或允许(1)取得队列数据总容量 */
#define EN_QUEUE_FLUSH            	1     	/* 禁止(0)或允许(1)清空队列           */

//回调函数
typedef uint8 	(*ReadEmpty)(void *ctx, void *data);
typedef uint8 	(*WriteFull)(void *queue, void *data, void *mod);

typedef struct _DataQueue
{
    uint8     *out;                   		/* 指向数据输出位置         			*/
    uint8     *in;                    		/* 指向数据输入位置         			*/
    uint8     *end;                   		/* 指向data_buf的结束位置   			*/
    uint16    data_num;               		/* 当前队列中数据个数       			*/
    uint16    data_len;               		/* 队列中允许存储的数据个数 			*/
    ReadEmpty readempty;			  		/* 读空处理回调函数         			*/
	WriteFull writefull;			  		/* 写满处理回调函数         			*/      
    uint8     data_buf[1];            		/* 存储数据的空间           			*/
}DataQueue;

uint8 queue_create(void *data_buf, uint32    buf_len, ReadEmpty readempty, WriteFull writefull);
/*********************************************************************************************************
** 函数名称: QueueCreate
** 功能描述: 初始化数据队列
** 输　入: data_buf      	：为队列分配的存储空间地址
**         SizeOfdata_buf	：为队列分配的存储空间大小（字节）
**         ReadEmpty		：为队列读空时处理程序
**         WriteFull		：为队列写满时处理程序
** 输　出: NOT_OK  			:参数错误
**         QUEUE_OK			:成功
********************************************************************************************************/

extern uint8 queue_read(uint8 *ret, void *data_buf);
/*********************************************************************************************************
** 函数名称: QueueRead
** 功能描述: 获取队列中的数据
** 输　入: ret				:存储返回的消息的地址
**         data_buf			:指向队列的指针
** 输　出: NOT_OK     		：参数错误
**         QUEUE_OK   		：收到消息
**         QUEUE_EMPTY		：队列空
********************************************************************************************************/

extern uint8 queue_write(void *data_buf, uint8 data);
/*********************************************************************************************************
** 函数名称: QueueWrite
** 功能描述: FIFO方式发送数据
** 输　入: data_buf 		:指向队列的指针
**         data				:发送的数据
** 输　出: NOT_OK   		：参数错误
**         QUEUE_FULL		:队列满
**         QUEUE_OK  		:发送成功
********************************************************************************************************/

extern uint8 queue_write_front(void *data_buf, uint8 data);
/*********************************************************************************************************
** 函数名称: QueueWrite
** 功能描述: LIFO方式发送数据
** 输　入: data_buf			:指向队列的指针
**         data				:消息数据
** 输　出: QUEUE_FULL		:队列满
**         QUEUE_OK			:发送成功
********************************************************************************************************/

extern uint16 queue_data_num(void *data_buf);
/*********************************************************************************************************
** 函数名称: Queuedata_num
** 功能描述: 取得队列中数据数
** 输　入: data_buf			:指向队列的指针
** 输　出: 消息数
********************************************************************************************************/

extern uint16 queue_size(void *data_buf);
/*********************************************************************************************************
** 函数名称: QueueSize
** 功能描述: 取得队列总容量
** 输　入: data_buf			:指向队列的指针
** 输　出: 队列总容量
********************************************************************************************************/

extern void queue_flush(void *data_buf);
/*********************************************************************************************************
** 函数名称: OSQFlush
** 功能描述: 清空队列
** 输　入: data_buf			:指向队列的指针
********************************************************************************************************/

#endif

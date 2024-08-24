#include "ringbuffer.h"

int rb_init(ring_buffer_t *rb, int size)
{	
	rb->RingBuffer=membag_alloc(size);
	rb->size=size;
	
	if(rb->RingBuffer==NULL)
		return(0);
		
	return(1);
}

int rb_has_data(ring_buffer_t *rb)
{
	if(rb->RingBufferHead!=rb->RingBufferTail)
		return(1);
	return(0);	
}

int rb_write(ring_buffer_t *rb,unsigned char ch)
{
	 rb->RingBuffer[rb->RingBufferHead]=ch;
	 rb->RingBufferHead++;
	 rb->RingBufferHead&=(rb->size-1);
	 
	 if(rb->RingBufferHead==rb->RingBufferTail) //buffer overflow
	 	return(0);
	return(1);		 
}

int rb_write_many(ring_buffer_t *rb,unsigned char *ch,int len)
{
	int i;
	
	for(i=0;i<len;i++)
	{
		if(rb_write(rb,ch[i])==0) //buffer overflow
			return(0);
	}
	return(1);
}

unsigned char rb_read(ring_buffer_t *rb)
{
	unsigned char retval;
	
	if(rb_has_data(rb))
	{			
		retval=rb->RingBuffer[rb->RingBufferTail];
		rb->RingBufferTail++;
		rb->RingBufferTail&=(rb->size-1);
		return(retval);
	}
	else
		return(0);
}

int rb_get_data_len(ring_buffer_t *rb)
{
	if(rb->RingBufferHead < rb->RingBufferTail)
		return((rb->RingBufferHead + rb->size) - rb->RingBufferTail);
	return((rb->RingBufferHead) - rb->RingBufferTail);
}

int rb_half_full(ring_buffer_t *rb)
{
	return(rb_get_data_len(rb) > (rb->size/2));
}
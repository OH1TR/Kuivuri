
/*
 * ringbuffer.h
 *
 * Created: 29.10.2013 19:14:25
 *  Author: Admin
 */ 
#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <membag.h>

typedef struct 
{ 
	unsigned int size; 
	volatile unsigned char volatile *RingBuffer; 
	volatile unsigned int RingBufferHead; 
	volatile unsigned int RingBufferTail; 
} ring_buffer_t;

int rb_init(ring_buffer_t *rb, int size);
int rb_has_data(ring_buffer_t *rb);
int rb_write(ring_buffer_t *rb,unsigned char ch);
int rb_write_many(ring_buffer_t *rb,unsigned char *ch,int len);
unsigned char rb_read(ring_buffer_t *rb);
int rb_get_data_len(ring_buffer_t *rb);
int rb_half_full(ring_buffer_t *rb);

#endif

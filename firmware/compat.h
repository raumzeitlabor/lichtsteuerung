/* 
 * File:   compat.h
 * Author: felicitus
 *
 * Created on May 24, 2013, 6:39 PM
 */

#ifndef COMPAT_H
#define	COMPAT_H

/**
   \ingroup deprecated_items
   \def sbi(port, bit)
   \deprecated

   Set \c bit in IO port \c port.
*/
#define sbi(port, bit) (port) |= (1 << (bit))

/**
   \ingroup deprecated_items
   \def cbi(port, bit)
   \deprecated

   Clear \c bit in IO port \c port.
*/
#define cbi(port, bit) (port) &= ~(1 << (bit))


#endif	/* COMPAT_H */


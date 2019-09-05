#ifndef PROTECTOR_H_
#define PROTECTOR_H_

#include <msp430g2553.h>
void ProtectorInit(); //P1.0 LED open when some problem happened.
void Require();
void Release();

#endif /* PROTECTOR_H_ */

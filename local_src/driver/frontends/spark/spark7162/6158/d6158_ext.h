#include <linux/dvb/frontend.h>
#include <linux/dvb/dvb_frontend.h>

extern struct dvb_frontend* dvb_d6158_attach(struct i2c_adapter* i2c,UINT8 system);

extern int  demod_d6158_Identify(struct i2c_adapter* i2c_adap,U8 ucID);


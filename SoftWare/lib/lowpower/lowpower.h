#ifndef _LOWPOWER_H_
#define _LOWPOWER_H_


#ifdef __cplusplus
extern "C" {

#define LOWPOWER_RUNTIME 10000

void lowpower_init(void);
bool lowpower_loop(void);

}
#endif
#endif /* _LOWPOWER_H_ */
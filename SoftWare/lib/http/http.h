#ifndef __HTTP_H__
#define __HTTP_H__


#ifdef __cplusplus
extern "C" {

void http_server_init(void);
void http_server_loop(void);
void http_lowpower_on(void);
void http_lowpower_off(void);

}
#endif /* __cplusplus */

#endif /* __HTTP_H__ */

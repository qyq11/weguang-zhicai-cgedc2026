#ifndef AUDIO_APP_H
#define AUDIO_APP_H

#include <stdint.h>

/* 音频应用层当前只保留初始化辅助函数，不保留运行期音频业务。 */
void audio_app_init(void);
void audio_app_scan_i2c(void);

#endif /* AUDIO_APP_H */

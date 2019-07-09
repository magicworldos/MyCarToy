#include <typedef.h>

#define PWM_MAX_VALUE	(2000)
#define PWM_MIN_VALUE	(1000)

#define PWM_SCALE		((PWM_MAX_VALUE - PWM_MIN_VALUE) / 2)
#define PWM_MID 		(PWM_MIN_VALUE + PWM_SCALE)

#define PWM_OFFSET_0	(0)
#define PWM_OFFSET_1	(0)
#define PWM_OFFSET_2	(0)
#define PWM_OFFSET_3	(0)
#define PWM_OFFSET_4	(0)
#define PWM_OFFSET_5	(0)
#define PWM_OFFSET_6	(0)
#define PWM_OFFSET_7	(0)

void pwm_init(void);

void pwm_set_values(uint16_t *pwm);

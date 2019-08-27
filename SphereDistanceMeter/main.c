#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <applibs/log.h>
#include <applibs/gpio.h>

int main(void)
{
	const struct timespec sleepTime = { 1, 0};

    int fd = GPIO_OpenAsOutput(9, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fd < 0) {
        Log_Debug(
            "Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
            strerror(errno), errno);
        return -1;
    }

	int trig = GPIO_OpenAsOutput(69, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (trig < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}

	int echo = GPIO_OpenAsInput(70);
	if (echo < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	//initialize the trigger
	GPIO_SetValue(trig, GPIO_Value_Low);
	nanosleep(&sleepTime, NULL);

	struct timespec pulse_start, pulse_end, pulse_duration;
	int val = 0;

	while (true)
	{
		//toggle the trigger high for 10us
		const struct timespec triggerTime = { 0, 1e4 };
		GPIO_SetValue(trig, GPIO_Value_High);
		nanosleep(&triggerTime, NULL);
		GPIO_SetValue(trig, GPIO_Value_Low);

		//get the start of pulse receive
		while (val == 0){
			GPIO_GetValue(echo, &val);
		}
		clock_gettime(CLOCK_REALTIME, &pulse_start);

		//get the end of pulse receive
		while (val == 1){
			GPIO_GetValue(echo, &val);
		}
		clock_gettime(CLOCK_REALTIME, &pulse_end);

		// Calculate time it took
		double pulse_duration = (pulse_end.tv_sec - pulse_start.tv_sec) +
		  					   (pulse_end.tv_nsec - pulse_start.tv_nsec) / 1e9;
		Log_Debug("Time: %lf\n", pulse_duration);

		double distance = pulse_duration * 17150;
		Log_Debug("Distance: %lf\n", distance);

		nanosleep(&sleepTime, NULL);
	}
}
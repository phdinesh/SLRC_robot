################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Analog_Sensor/analog_sensor.cpp 

OBJS += \
./Analog_Sensor/analog_sensor.o 

CPP_DEPS += \
./Analog_Sensor/analog_sensor.d 


# Each subdirectory must supply rules for building sources it contributes
Analog_Sensor/%.o: ../Analog_Sensor/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"E:\D_Backup\Tech\arduino-1.0.1-windows\arduino-1.0.1\hardware\arduino\variants\mega" -I"E:\D_Backup\Tech\arduino-1.0.1-windows\arduino-1.0.1\hardware\arduino\cores\arduino" -I"E:\D_Backup\Tech\arduino-1.0.1-windows\arduino-1.0.1\libraries" -Wall -Os -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions --pedantic -c -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



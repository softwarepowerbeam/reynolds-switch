################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app.c \
../Core/Src/button.c \
../Core/Src/deadline_timer.c \
../Core/Src/gpio.c \
../Core/Src/led_indicator.c \
../Core/Src/lights.c \
../Core/Src/main.c \
../Core/Src/motion_light_driver.c \
../Core/Src/pyd15X8.c \
../Core/Src/relay_l114fl_drv8210.c \
../Core/Src/stm32c0xx_hal_msp.c \
../Core/Src/stm32c0xx_it.c \
../Core/Src/switch_selector.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32c0xx.c \
../Core/Src/tim.c 

OBJS += \
./Core/Src/app.o \
./Core/Src/button.o \
./Core/Src/deadline_timer.o \
./Core/Src/gpio.o \
./Core/Src/led_indicator.o \
./Core/Src/lights.o \
./Core/Src/main.o \
./Core/Src/motion_light_driver.o \
./Core/Src/pyd15X8.o \
./Core/Src/relay_l114fl_drv8210.o \
./Core/Src/stm32c0xx_hal_msp.o \
./Core/Src/stm32c0xx_it.o \
./Core/Src/switch_selector.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32c0xx.o \
./Core/Src/tim.o 

C_DEPS += \
./Core/Src/app.d \
./Core/Src/button.d \
./Core/Src/deadline_timer.d \
./Core/Src/gpio.d \
./Core/Src/led_indicator.d \
./Core/Src/lights.d \
./Core/Src/main.d \
./Core/Src/motion_light_driver.d \
./Core/Src/pyd15X8.d \
./Core/Src/relay_l114fl_drv8210.d \
./Core/Src/stm32c0xx_hal_msp.d \
./Core/Src/stm32c0xx_it.d \
./Core/Src/switch_selector.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32c0xx.d \
./Core/Src/tim.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C011xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app.cyclo ./Core/Src/app.d ./Core/Src/app.o ./Core/Src/app.su ./Core/Src/button.cyclo ./Core/Src/button.d ./Core/Src/button.o ./Core/Src/button.su ./Core/Src/deadline_timer.cyclo ./Core/Src/deadline_timer.d ./Core/Src/deadline_timer.o ./Core/Src/deadline_timer.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/led_indicator.cyclo ./Core/Src/led_indicator.d ./Core/Src/led_indicator.o ./Core/Src/led_indicator.su ./Core/Src/lights.cyclo ./Core/Src/lights.d ./Core/Src/lights.o ./Core/Src/lights.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/motion_light_driver.cyclo ./Core/Src/motion_light_driver.d ./Core/Src/motion_light_driver.o ./Core/Src/motion_light_driver.su ./Core/Src/pyd15X8.cyclo ./Core/Src/pyd15X8.d ./Core/Src/pyd15X8.o ./Core/Src/pyd15X8.su ./Core/Src/relay_l114fl_drv8210.cyclo ./Core/Src/relay_l114fl_drv8210.d ./Core/Src/relay_l114fl_drv8210.o ./Core/Src/relay_l114fl_drv8210.su ./Core/Src/stm32c0xx_hal_msp.cyclo ./Core/Src/stm32c0xx_hal_msp.d ./Core/Src/stm32c0xx_hal_msp.o ./Core/Src/stm32c0xx_hal_msp.su ./Core/Src/stm32c0xx_it.cyclo ./Core/Src/stm32c0xx_it.d ./Core/Src/stm32c0xx_it.o ./Core/Src/stm32c0xx_it.su ./Core/Src/switch_selector.cyclo ./Core/Src/switch_selector.d ./Core/Src/switch_selector.o ./Core/Src/switch_selector.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32c0xx.cyclo ./Core/Src/system_stm32c0xx.d ./Core/Src/system_stm32c0xx.o ./Core/Src/system_stm32c0xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su

.PHONY: clean-Core-2f-Src


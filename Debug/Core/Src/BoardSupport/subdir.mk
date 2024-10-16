################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/BoardSupport/button.c \
../Core/Src/BoardSupport/led_indicator.c \
../Core/Src/BoardSupport/lights.c \
../Core/Src/BoardSupport/pyd15X8.c \
../Core/Src/BoardSupport/relay_l114fl_drv8210.c \
../Core/Src/BoardSupport/switch_selector.c 

OBJS += \
./Core/Src/BoardSupport/button.o \
./Core/Src/BoardSupport/led_indicator.o \
./Core/Src/BoardSupport/lights.o \
./Core/Src/BoardSupport/pyd15X8.o \
./Core/Src/BoardSupport/relay_l114fl_drv8210.o \
./Core/Src/BoardSupport/switch_selector.o 

C_DEPS += \
./Core/Src/BoardSupport/button.d \
./Core/Src/BoardSupport/led_indicator.d \
./Core/Src/BoardSupport/lights.d \
./Core/Src/BoardSupport/pyd15X8.d \
./Core/Src/BoardSupport/relay_l114fl_drv8210.d \
./Core/Src/BoardSupport/switch_selector.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/BoardSupport/%.o Core/Src/BoardSupport/%.su Core/Src/BoardSupport/%.cyclo: ../Core/Src/BoardSupport/%.c Core/Src/BoardSupport/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C011xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-BoardSupport

clean-Core-2f-Src-2f-BoardSupport:
	-$(RM) ./Core/Src/BoardSupport/button.cyclo ./Core/Src/BoardSupport/button.d ./Core/Src/BoardSupport/button.o ./Core/Src/BoardSupport/button.su ./Core/Src/BoardSupport/led_indicator.cyclo ./Core/Src/BoardSupport/led_indicator.d ./Core/Src/BoardSupport/led_indicator.o ./Core/Src/BoardSupport/led_indicator.su ./Core/Src/BoardSupport/lights.cyclo ./Core/Src/BoardSupport/lights.d ./Core/Src/BoardSupport/lights.o ./Core/Src/BoardSupport/lights.su ./Core/Src/BoardSupport/pyd15X8.cyclo ./Core/Src/BoardSupport/pyd15X8.d ./Core/Src/BoardSupport/pyd15X8.o ./Core/Src/BoardSupport/pyd15X8.su ./Core/Src/BoardSupport/relay_l114fl_drv8210.cyclo ./Core/Src/BoardSupport/relay_l114fl_drv8210.d ./Core/Src/BoardSupport/relay_l114fl_drv8210.o ./Core/Src/BoardSupport/relay_l114fl_drv8210.su ./Core/Src/BoardSupport/switch_selector.cyclo ./Core/Src/BoardSupport/switch_selector.d ./Core/Src/BoardSupport/switch_selector.o ./Core/Src/BoardSupport/switch_selector.su

.PHONY: clean-Core-2f-Src-2f-BoardSupport


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Application/motion_light_driver.c \
../Core/Src/Application/nutone_app.c 

OBJS += \
./Core/Src/Application/motion_light_driver.o \
./Core/Src/Application/nutone_app.o 

C_DEPS += \
./Core/Src/Application/motion_light_driver.d \
./Core/Src/Application/nutone_app.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Application/%.o Core/Src/Application/%.su Core/Src/Application/%.cyclo: ../Core/Src/Application/%.c Core/Src/Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C011xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Application

clean-Core-2f-Src-2f-Application:
	-$(RM) ./Core/Src/Application/motion_light_driver.cyclo ./Core/Src/Application/motion_light_driver.d ./Core/Src/Application/motion_light_driver.o ./Core/Src/Application/motion_light_driver.su ./Core/Src/Application/nutone_app.cyclo ./Core/Src/Application/nutone_app.d ./Core/Src/Application/nutone_app.o ./Core/Src/Application/nutone_app.su

.PHONY: clean-Core-2f-Src-2f-Application


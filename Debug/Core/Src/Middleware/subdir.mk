################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Middleware/deadline_timer.c 

OBJS += \
./Core/Src/Middleware/deadline_timer.o 

C_DEPS += \
./Core/Src/Middleware/deadline_timer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Middleware/%.o Core/Src/Middleware/%.su Core/Src/Middleware/%.cyclo: ../Core/Src/Middleware/%.c Core/Src/Middleware/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C011xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Middleware

clean-Core-2f-Src-2f-Middleware:
	-$(RM) ./Core/Src/Middleware/deadline_timer.cyclo ./Core/Src/Middleware/deadline_timer.d ./Core/Src/Middleware/deadline_timer.o ./Core/Src/Middleware/deadline_timer.su

.PHONY: clean-Core-2f-Src-2f-Middleware


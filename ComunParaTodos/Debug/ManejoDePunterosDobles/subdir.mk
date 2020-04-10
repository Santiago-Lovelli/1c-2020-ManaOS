################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ManejoDePunterosDobles/ManejoDePunterosDobles.c 

OBJS += \
./ManejoDePunterosDobles/ManejoDePunterosDobles.o 

C_DEPS += \
./ManejoDePunterosDobles/ManejoDePunterosDobles.d 


# Each subdirectory must supply rules for building sources it contributes
ManejoDePunterosDobles/%.o: ../ManejoDePunterosDobles/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Serializacion/Serializacion.c 

OBJS += \
./Serializacion/Serializacion.o 

C_DEPS += \
./Serializacion/Serializacion.d 


# Each subdirectory must supply rules for building sources it contributes
Serializacion/%.o: ../Serializacion/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



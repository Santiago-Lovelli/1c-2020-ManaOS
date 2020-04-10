################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sac-Server/Sac-Server.c 

OBJS += \
./Sac-Server/Sac-Server.o 

C_DEPS += \
./Sac-Server/Sac-Server.d 


# Each subdirectory must supply rules for building sources it contributes
Sac-Server/%.o: ../Sac-Server/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



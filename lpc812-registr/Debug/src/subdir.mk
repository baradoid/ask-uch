################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cr_cpp_config.cpp \
../src/cr_startup_lpc8xx.cpp \
../src/main.cpp \
../src/utils.cpp 

C_SRCS += \
../src/crp.c \
../src/mtb.c 

OBJS += \
./src/cr_cpp_config.o \
./src/cr_startup_lpc8xx.o \
./src/crp.o \
./src/main.o \
./src/mtb.o \
./src/utils.o 

C_DEPS += \
./src/crp.d \
./src/mtb.d 

CPP_DEPS += \
./src/cr_cpp_config.d \
./src/cr_startup_lpc8xx.d \
./src/main.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -D__NEWLIB__ -DDEBUG -DCORE_M0PLUS -D__MTB_DISABLE -D__MTB_BUFFER_SIZE=256 -D__LPC8XX__ -I"C:\Users\Dmitry\Documents\LPCXpresso_7.8.0_426\askue\lpc812-workspace\lpc_chip_8xx\inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__NEWLIB__ -DDEBUG -DCORE_M0PLUS -D__MTB_DISABLE -D__MTB_BUFFER_SIZE=256 -D__LPC8XX__ -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



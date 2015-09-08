################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: lpc812-registr.axf

# Tool invocations
lpc812-registr.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU C++ Linker'
	arm-none-eabi-c++ -nostdlib -L"C:\Users\Dmitry\Documents\LPCXpresso_7.8.0_426\askue\lpc812-workspace\lpc_chip_8xx\Debug" -Xlinker -Map="lpc812-registr.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "lpc812-registr_Debug.ld" -o "lpc812-registr.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) lpc812-registr.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "lpc812-registr.axf" ; arm-none-eabi-objcopy -O ihex lpc812-registr.axf lpc812-registr.hex ; # arm-none-eabi-objcopy -v -O binary "lpc812-registr.axf" "lpc812-registr.bin" ; # checksum -p LPC812 -d "lpc812-registr.bin" ; # arm-none-eabi-objcopy -v -O binary "lpc812-registr.axf" "lpc812-registr.bin" ; # checksum -p LPC812 -d "lpc812-registr.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

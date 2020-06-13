################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Clock.obj: /Users/abdallah/Downloads/UCDAVIS_EEC10_Lab8_DL_Final_Project/inc/Clock.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/abdallah/Downloads/UCDAVIS_EEC10_Lab8_DL_Final_Project/UCDAVIS_EEC10_Lab8_DL_Final_Project" --include_path="/Applications/ti/ccs1000/ccs/ccs_base/arm/include/" --include_path="/Applications/ti/ccs1000/ccs/ccs_base/arm/include/CMSIS" --include_path="/Users/abdallah/Downloads/UCDAVIS_EEC10_Lab8_DL_Final_Project/driverlib/MSP432P4xx" --include_path="/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include/" --include_path="/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SysTick.obj: /Users/abdallah/Downloads/UCDAVIS_EEC10_Lab8_DL_Final_Project/inc/SysTick.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/abdallah/Downloads/UCDAVIS_EEC10_Lab8_DL_Final_Project/UCDAVIS_EEC10_Lab8_DL_Final_Project" --include_path="/Applications/ti/ccs1000/ccs/ccs_base/arm/include/" --include_path="/Applications/ti/ccs1000/ccs/ccs_base/arm/include/CMSIS" --include_path="/Users/abdallah/Downloads/UCDAVIS_EEC10_Lab8_DL_Final_Project/driverlib/MSP432P4xx" --include_path="/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include/" --include_path="/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/abdallah/Downloads/UCDAVIS_EEC10_Lab8_DL_Final_Project/UCDAVIS_EEC10_Lab8_DL_Final_Project" --include_path="/Applications/ti/ccs1000/ccs/ccs_base/arm/include/" --include_path="/Applications/ti/ccs1000/ccs/ccs_base/arm/include/CMSIS" --include_path="/Users/abdallah/Downloads/UCDAVIS_EEC10_Lab8_DL_Final_Project/driverlib/MSP432P4xx" --include_path="/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include/" --include_path="/Applications/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



# finding perfect sens

import math

# input: 
    # base sens
    # too low
    # too high
    # perfect
# output
    # current sens to test
    # perfect sens

# ask the user for base number
def askInputLoop(prompt, func, err, valid_inputs = {}):
    # continous loop to ask for a float value
    while True:
        try:
            # try asking for a number. if it is valid float, return
            user_input = func(prompt)
            if ((valid_inputs != {}) and (user_input not in valid_inputs)):
                raise err
            return user_input
        # if not valid float, print error message, loop continues
        except err:
            print("Try again with a valid value.")
        
def askFloatUserInput(prompt):
    return float(input(prompt))

def askStringUserInput(prompt):
    return input(prompt).strip().lower()
        
# get base number
base_number_prompt = "Please enter a base number/decimal value: "
base_number = askInputLoop(base_number_prompt, askFloatUserInput, ValueError)

# setting default values
current_number = base_number
low_number = float(0)
high_number = base_number

# set a low value
low_number_prompt = "Please enter a MIN number/decimal value: "
low_number = askInputLoop(low_number_prompt, askFloatUserInput, ValueError)

# set a max value
high_number_prompt = "Please enter a MAX number/decimal value: "
high_number = askInputLoop(high_number_prompt, askFloatUserInput, ValueError)


# ask user if number is too low or too high
while True:
    high_low_prompt = 'Please specify whether current value was "low" (1) or "high" (h) or "perfect" (p): '
    valid_inputs = {"low", "l", "high", "h", "perfect", "p"}

    # ask if number is too low or high
    high_low = askInputLoop(high_low_prompt, askStringUserInput, ValueError, valid_inputs=valid_inputs)
    match high_low:
    # scenario 1: too low
        case "l" | "low":
            print("Too low, adjusting value: ")
            low_number = current_number
    # scenario 2: too high
        case "h" | "high":
            print("Too high, adjusting value:")
            high_number = current_number
    # scenario 3: perfect
        case "p" | "perfect":
            break
        case  _:
            print("Match/case error")
    current_number = (high_number - low_number) / 2 + low_number
    print("Current Number: " + str(current_number))


print("Perfect Number: " + str(current_number))
print("Low Number: " + str(low_number))
print("High Number: " + str(high_number))
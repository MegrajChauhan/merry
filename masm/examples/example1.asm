; comments
; The structure for a program is simple really
; Every program must start with "entry"
; The instructions will be just a fancy sugarcoat for the binary so there won't really be much abstraction

; variable declarations
db var_name 100

; The format is db|dw|dd|dq variable_name expression|string|array
; The expression is evaluated
; In case of db the expression can be replaced with a string "some string"
; the expression is a mathematical expression that may include +,-,*,/,%,>,<,>=,<=,==,!=,~(NOT),(),&,|,&&,||,^(XOR)
; arrays are created as expression1, expression2, expression3, expression4,....
; Every boolean operation generates 0 for false and 1 for true, thus, can be used for mathematical evaluation
; Any expression can include other variables as well
; Thus the following statements would be true

db msg "Hello World!", 10 ; 10 just means '\n'
dw array 10,20,100,0b10011011,0o87, 0x1234
dq all_of_it array, 100 + 12 - (89 >> 3), 1

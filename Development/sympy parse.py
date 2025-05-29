import sympy as smp
import queue
from sympy.parsing.sympy_parser import parse_expr

class Computed_Variables_List:
    def __init__(self, k = 1):
        self.__variables = list()

    def __apply_function(self, f, values, var_name):
        values[var_name] = f.evalf(subs = values)

    def __call__(self, values):
        for (var_name, f) in self.__variables:
            self.__apply_function(f, values, var_name)
        return values

    def from_txt(self, filename):
        file = open(filename)
        for str_function in file.readlines():
            if str_function[0] == "#":
                continue
            try:
                # let str_function = "V = function(values)"
                eq_index = str_function.find("=")
                var_name = str_function[:eq_index - 1]    # var_name = "V"
                f = parse_expr(str_function[eq_index+1:]) # f = function
                self.__variables.append((var_name, f))
            except Exception as e:
                print('"'+str_function+'"', "ошибка:", e)
        file.close()
        return

def from_txt(filename):
    cvl = Computed_Variables_List()
    cvl.from_txt(filename)
    return cvl

if __name__ == "__main__":
    cvl = from_txt("sympy parse.txt")
    print("created CVL")
    values = {'CH0' : 0, 'CH1' : 1,
              'CH2' : 2, 'CH3' : 3}
    print(cvl(values))

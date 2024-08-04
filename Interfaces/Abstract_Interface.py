class Abstract_Interface:
    def __init__(self):
        self.myUIElements = []

    def __setattr__(self, key, value):
        print("Trying to set to interface ",
              type(self), type(key), type(value))
        return

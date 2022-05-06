# ValueOrError
Header only library that defines a class that holds either a value or an error value. Intended to be used as the return type of functions where the full range of possible values is necessary, thus avoiding arbitrarily chosen values as indicatives of errors. Can be used in a freestanding environment if <type_traits> is available.

## Usage

```cpp
#include "valueorerror.hpp"

hls::ValueOrError<int, int> my_nice_function(int i) {
  if(i == 10) {
    return hls::value(i);
  }
  
  return hls::error(i);
}

int main() {
  auto a = my_nice_function(10);
  
  if(a.is_error()) {
    // Oh no, we got an error
    auto error_code = a.get_error();
    // Do stuff with the error code...
    return -1;
  }
  
  auto value = a.get_value();
  // Do stuff with the value
  
  return 0;
}
```

## Reasoning behind it

There are multiple ways of handling errors in C++. There are exceptions, error values, global variables and others. Exceptions are not available in a freestanding environment and some code bases simply don't allow them. Error values is what has mostly been used in C code and some C++ code, specially code that has to interact with C libraries. Global variables indicating errors introduce unnecessary complexity in code, an example would be handling different threads. Although this is not a magic bullet, it provides a good way of letting the function handle whether a value is an error or not, allows us to have the full range of a given type as valid return values and makes it (somewhat) easier to split error handling code from regular code.

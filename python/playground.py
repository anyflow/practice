import device_id.temp as tempControl
import math

numbers = [0, 1, 2, 'three', 4, 5, 6, 7, 8, 9]

print range(len(numbers))

print [math.factorial(num) for num in range(5) if num % 2 != 0]

print [num for num in xrange(10 ** 2) if num % 2 != 0]


def a_function(arg1, arg2="default", *args, **kwargs):
    """This is a short piece of documentation for this function.
       It can span multiple lines.
    """
    print "arg1:", arg1     # arg1 is a mandatory parameter
    print "arg2:", arg2     # arg2 is an optional parameter with a default value
    print "args:", args     # args is a tuple of positional parameters
    print "kwargs:", kwargs  # kwargs is a dictionary of keyword parameters


help(a_function)


def multiReturn():
    """Returns a tuple
    """
    return 10, 20, 'thirty'


print multiReturn()

a, b, c = multiReturn()

print a, b, c


def ternary(a, b, c):
    print a, b, c


kwargs = {'a': 1, 'b': 2, 'c': 3}

ternary(**kwargs)

print '==================================================='


class Person(object):
    def __init__(self, first, last):
        self.first = first
        self.last = last

    def full_name(self):
        return '%s %s' % (self.first, self.last)

    def __str__(self):
        return 'Person: ' + self.full_name()


person = Person('Hyunjeong', 'Park')

print person

print '---------------------------------------------------'


class SuperHero(Person):
    """SuperHero"""

    def __init__(self, first, last, nick):
        super(SuperHero, self).__init__(first, last)
        self.nick = nick

    def nick_name(self):
        return 'I am %s' % self.nick


p = SuperHero('Hyunjeong', 'Park', 'anyflow')

print p.nick_name()

print type(p)

print '---------------------------------------------------'

tempControl.printTemp()

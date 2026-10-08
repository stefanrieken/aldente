# Pasta al Dente
A bytecode based Pasta implementation for general (and possibly shell) purposes.

Current status: can run a precompiled test expression.

## Common Pasta features
- Simple but unambiguous one-pass syntax
- No garbage collection required

## Virtual machine
Call system / (internal) vm similar to what is used in Sgetti, i.e.:
- Bytecode VM uses both core commands and built-in primitives
- Use Pasta's tactic to delegate evaluation to 8 prim groups of max 32 entries,
  the first group now representing the core commands
- Expressions are always primitive underwater (e.g.: `call myfunc arg1 arg2`)
- Core commands are (tag-)typed so that arguments will be runtime (tag-)typed
  (TODO will we use this?)

## Frames
Memory usage is centered around the use of Frames.
Frames have a parent pointer structure.

### Stack frames
When used as a stack frame, Frames are allocated and released according to the
lifetime of the underlying function or block.

As per previous implementations, `bind` creates an anonymous variable with the
block argument as its value, binding the function block to the current state.
As this binding will not outlive the current context, this allows for callbacks
but not call-forwards:

        foo (bind { args x:; print x }); # pass as callback only

An alternative is to define a function globally, or within a namespace (TODO obtainable?).

### Compound values
Frames can also be used as compound value objects.

To control the lifetime of a frame, it must have a known owner; but its owner
must also have a link back to it. A practical solution may be to always access
such objects through an anonymous stack value, as with closures (`bind`).

(Presently this releases us from the decision whether a compound value frame
must also have a parent pointer, and if yes, what for.)

Frames may be reparented by clearing the pointer in the original stack value.

(Syntax still to be envisioned. In theory this is equivalent to what Rust does,
but for now I expect a dynamic syntax rather than compile time enforcement.)

#### Compound value syntax
Compound value frame definition syntax can be fully dynamic, just like block
definitions:

        foo (struct { define x: 42; define y: 33; });

In theory we can also dynamically access frame contents, like so:

        print "point.x is:" (get-at point x:);

However, this can only be un-ambiguously compiled in case of lookup-by-name.
If instead we translate labels to offsets, we cannot easily determine whether
`x:` refers to an offset from the current stack frame, or from `point`.

So this requires a bit of extra syntax; and indeed the dot operator seems to
be the natural candidate:

        print "point.x is: " point.x;

Even then, for `.x` to be defined in terms of offsets, means that we must have
a compile time (= static, or at least virtual) model of `point`, and apply it
accordingly. Then again, we don't need to, and can maintain lookup-by-name for
compound objects.

Under the hood, "point.x" should translate to two core primitives:

        REF point # lookup variable 'point' and push its value
        SLOT x    # pull 'point', lookup `x` there and push its value

This same mechanism can also be used to explicitly represent parent variable
access, which is necessary for lookup by offset:

       define x: 42;
       if true { print x; }; # actually: print parent.x;

(Note: while Pasta's syntax _provisions_ for indexed access, lookup-by-name
is still the pragmatic approach, and not as slow as it sounds: at least the
names will be reduced to numbers at runtime.)

#### Objects?
At this point we may imagine ourselves halfway towards an OO system, but I
can assure you that it will be the smaller half yet.

So far we have opened our object's data, the part that some scholars insist
be kept hidden; but we haven't even provided a way communicate its class, let
alone to resolve the ambiguity of referencing both using the same syntax (if
C++ is any model to go by).

Still, this may put us on the right track after all; and if we can learn to
manage compound objects without a garbage collector, I'm sure that experience
will not be fully wasted on designing an OO system.

### Stack modeling
By tracking statements like `args` and `define` at parse time, we can produce
a model of the runtime stack at that point in code.

This can be used to calculate variable offsets for access-by-offset, and even
to substitute predictable values for constants. While completely optional for
access-by-name, the easiest approach to stack modeling is to just have the same
excessively "rich" stack both at parse time and runtime.


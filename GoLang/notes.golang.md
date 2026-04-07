# The pull-request

> https://github.com/markus-wa/demoinfocs-golang/pull/640

# Public c++ work:

[ ] CatPi https://github.com/NeptuneMagicSauce/CatPi
- hardware integration: GPIO, electronic relay, load cell, voltage amplifier
- front-end: touch screen, Qt 6 Widgets
- CMake
- decoupling: most modules are fully independent, orchestrated through Qt signals
- modern cpp: almost-always-auto, STL algorithms, cpp modules (not fully supported)
- throttling, hysteresis, throughput logic

[ ] AppQt https://github.com/NeptuneMagicSauce/AppQt
toy project illustrating CMake and Qt integration

[ ] DotFiles https://github.com/NeptuneMagicSauce/dotfiles
curated `.bashrc`, `.emacs.el`, `.gdbinit`, `.gitconfig`, `.tmux.conf`

# GoLang: what I learned

[ ] It's compiled, but `go run` makes it looks like it's a scripting language.

So, both performant and convenient.

[ ] Declaration of variables looks like assignment, except it is `:=` rather than `=` on assignment.

[ ] Tooling is great
formatter is built-in: `gofmt`, integrated in the emacs IDE on save
compiler is integrated with Language Server Protocol through `gopls` in the IDE
linter `golangci-lint`, with `/.golangci.yml` is integrated in the IDE

And so I have in the IDE:
- no unused variable nor parameter
- no unhandled errors
- no snake case, only camel case

[ ] Garbage-Collected: I've been managing the manager from C# times:

If I want to control the releasing of memory ...

For performance reasons, in order to gain back control, I could want to choose when the de-allocations happen ...

I know a few ways to do it:

- pool pre-allocate: I've rolled out my custom implementation, I understand go has `sync.Pool`
- arena allocator: I've read theory on this, not yet used or implemented
- store a 'final' copy of each pointer in a collection, so that I have the last copy
and then remove this final reference when I want to trigger the releasing
such as:
> cpp
```cpp
#include <bits/stdc++.h>

using namespace std;

int main(int, char **) {

  auto deferredReleases = list<shared_ptr<int>>{};

  // producer:
  deferredReleases.push_back(make_shared<int>(42));
  deferredReleases.push_back(make_shared<int>(43));

  static auto constexpr N = 5UL;
  // called when we can afford N releases
  for (auto index : views::iota(0UL, min(N, deferredReleases.size()))) {
    // pop the oldest item:
    deferredReleases.pop_front();
    cout << "to release: " << deferredReleases.size() << endl;
  }
  return 0;
}
```

> golang
```go
package main

import "container/list"
import "fmt"

func main() {

    deferredReleases := list.New()

    // producer:
    deferredReleases.PushBack(make([]byte, 1024))
    deferredReleases.PushBack(make([]byte, 1024))

    const N = 5

    // called when we can afford N releases
    for range min(N, deferredReleases.Len()) {
        // pop the oldest item:
        deferredReleases.Remove(deferredReleases.Front())
        fmt.Println("to release: ", deferredReleases.Len())
    }
}
```

But then maybe garbage collection is fast in Go and we do not need these gymnastics?

[ ] Error handling is great, I can not fail to verify the error code with the compiler and the linter

As I've observed:
> G104: Errors unhandled (gosec)

[ ] Unit Test: support is built-in
I write unit tests and functional tests that not only cover all branches,
but also run through all combinations of input data

As illustrated in the pull-request:
- I had a CI failure after my new unit test passed
- Its root cause was "when capturing stdout and stderr, they must be restored at the end of the unit test"
- This was not done in any other unit test of the repo
- Thus I could not see how it should be done from existing tests
- These previous unit tests did not verify the output was as expected
- Mine did

[ ] The `defer` keyword is a great implementation, very convenient usage

[ ] The `fmt` module has the same syntax as its c++ namesake

[ ] The `_` idiom for unused return values has the same syntax I learned with c++ third-party test frameworks: CTest, Google Test

CXX := g++
CXXFLAGS := -std=c++17
#CPPFLAGS := -DDEBUG
DEPFLAGS := -MMD -MP

BUILDDIR := build

OBJECTS := $(patsubst effects/%.cpp,$(BUILDDIR)/lib/%.o,$(wildcard effects/*.cpp))
TESTS := $(patsubst test/%.cpp,$(BUILDDIR)/test/%,$(wildcard test/*.cpp))

DEPS := $(patsubst %.o,%.d,$(OBJECTS)) $(patsubst %,%.d,$(TESTS))

$(shell mkdir -p $(BUILDDIR))
$(shell mkdir -p $(BUILDDIR)/lib)
$(shell mkdir -p $(BUILDDIR)/test)

.PHONY: test lib clean

test: $(TESTS)
	@for i in $(TESTS); do echo "Running $$i..."; $$i; done

$(TESTS):$(BUILDDIR)/test/%: test/%.cpp $(BUILDDIR)/effects.a
	$(CXX) -I. $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -o $@ $< $(BUILDDIR)/effects.a

lib: $(BUILDDIR)/effects.a

$(BUILDDIR)/effects.a: $(OBJECTS)
	@rm -f $(BUILDDIR)/effects.a
	ar rcs $(BUILDDIR)/effects.a $(OBJECTS)

$(OBJECTS):$(BUILDDIR)/lib/%.o: effects/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<

clean:
	@rm -rf $(BUILDDIR)

-include $(DEPS)

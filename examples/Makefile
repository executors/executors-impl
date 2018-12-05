DIRS = \
	actor \
	adapter \
	async_ops \
	properties \
	inline \
	nested \
	simple \
	pipeline

DISABLED = \
	async \
	invoke \
	for_each

.PHONY: all clean

all:
	$(foreach DIR,$(DIRS),make -C $(DIR) $(MAKEFLAGS) && )true

clean:
	$(foreach DIR,$(DIRS),make -C $(DIR) $(MAKEFLAGS) clean && )true

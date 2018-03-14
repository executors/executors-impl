DIRS = \
	actor \
	adapter \
	async \
	async_ops \
	exception \
	for_each \
	properties \
	inline \
	invoke \
	nested \
	simple \
	pipeline

.PHONY: all clean

all:
	$(foreach DIR,$(DIRS),make -C $(DIR) $(MAKEFLAGS) && )true

clean:
	$(foreach DIR,$(DIRS),make -C $(DIR) $(MAKEFLAGS) clean && )true

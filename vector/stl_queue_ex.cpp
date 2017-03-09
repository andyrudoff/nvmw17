#include <unistd.h>
#include <vector>

#include <libpmemobj++/allocator.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

namespace nvobj = nvml::obj;

namespace
{

/* available queue operations */
enum queue_op {
	UNKNOWN_QUEUE_OP,
	QUEUE_PUSH,
	QUEUE_POP,
	QUEUE_SHOW,

	MAX_QUEUE_OP,
};

/* queue operations strings */
const char *ops_str[MAX_QUEUE_OP] = {"", "push", "pop", "show"};

/*
 * parse_queue_op -- parses the operation string and returns matching queue_op
 */
queue_op
parse_queue_op(const char *str)
{
	for (int i = 0; i < MAX_QUEUE_OP; ++i)
		if (strcmp(str, ops_str[i]) == 0)
			return (queue_op)i;

	return UNKNOWN_QUEUE_OP;
}

int
file_exists(char const *file)
{
	return access(file, F_OK);
}

/* Only these two lines are "the implementation" */
using my_type = nvobj::p<long long>;
using my_container = std::vector<my_type, nvobj::allocator<my_type>>;

struct root {
	nvobj::persistent_ptr<my_container> cont;
};

}

int
main(int argc, char *argv[])
{
	if (argc < 3) {
		std::cerr << "usage: " << argv[0]
			  << " file-name [push [value]|pop|show]" << std::endl;
		return 1;
	}

	const char *path = argv[1];

	queue_op op = parse_queue_op(argv[2]);

	nvobj::pool<root> pop;

	if (file_exists(path) != 0) {
		pop = nvobj::pool<root>::create(
			path, "layout", PMEMOBJ_MIN_POOL, S_IWUSR | S_IRUSR);
		nvobj::transaction::exec_tx(pop, [&pop] {
			pop.get_root()->cont = nvobj::make_persistent<my_container>();
		});
	} else {
		pop = nvobj::pool<root>::open(path, "layout");
	}

	nvobj::transaction::exec_tx(pop, [&pop, &op, &argv] {
		auto q = pop.get_root()->cont;
		switch (op) {
			case QUEUE_PUSH:
				q->push_back(atoll(argv[3]));
				break;
			case QUEUE_POP:
				std::cout << q->back() << std::endl;
				q->pop_back();
				break;
			case QUEUE_SHOW:
				for(const auto &val : *q)
					std::cout << val << std::endl;
				break;
			default:
				throw std::invalid_argument("invalid queue operation");
				break;
		}
	});

	pop.close();

	return 0;
}

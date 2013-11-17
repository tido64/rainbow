// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

const char Rainbow_lua[] =

"local assert = assert\n"
"local getmetatable = getmetatable\n"
"local table_remove = table.remove\n"

/* Rainbow bootstrap */

"local __module_count = 1\n"
"local __modules = { nil }\n"

"function __update(dt)\n"
	"for i = 1, __module_count do\n"
		"__modules[i](dt)\n"
	"end\n"
	"update(dt)\n"
"end\n"

"local function get(module)\n"
	"local module_update = getmetatable(module).__update\n"
	"assert(module_update, \"Invalid module: Missing __update() function\")\n"
	"return module_update\n"
"end\n"

"rainbow.module = {\n"
	"register = function(module)\n"
		"local module_update = get(module)\n"
		"for i = 1, __module_count do\n"
			"if __modules[i] == module_update then\n"
				"return module\n"
			"end\n"
		"end\n"
		"__module_count = __module_count + 1\n"
		"__modules[__module_count] = module_update\n"
		"return module\n"
	"end,\n"
	"unregister = function(module)\n"
		"local module_update = get(module)\n"
		"for i = 1, __module_count do\n"
			"if __modules[i] == module_update then\n"
				"table_remove(__modules, i)\n"
				"__module_count = __module_count - 1\n"
				"return true\n"
			"end\n"
		"end\n"
	"end\n"
"}\n"

/* rainbow.input */

"local Input = rainbow.input\n"

"local __listener_count = 0\n"
"local __listeners = {}\n"

"function Input.subscribe(obj)\n"
	"__listener_count = __listener_count + 1\n"
	"__listeners[__listener_count] = obj\n"
"end\n"

"function Input.unsubscribe(obj)\n"
	"for i = __listener_count, 1, -1 do\n"
		"if __listeners[i] == obj then\n"
			"__listeners[i] = __listeners[__listener_count]\n"
			"__listeners[__listener_count] = nil\n"
			"__listener_count = __listener_count - 1\n"
			"break\n"
		"end\n"
	"end\n"
"end\n"

"function Input.unsubscribe_all()\n"
	"for i = 1, __listener_count do\n"
		"__listeners[i] = nil\n"
	"end\n"
	"__listener_count = 0\n"
"end\n"

"function Input.key_down(keys)\n"
	"local key, mod = keys[1], keys[2]\n"
	"for i = __listener_count, 1, -1 do\n"
		"__listeners[i]:key_down(key, mod)\n"
	"end\n"
"end\n"

"function Input.key_up(keys)\n"
	"local key, mod = keys[1], keys[2]\n"
	"for i = __listener_count, 1, -1 do\n"
		"__listeners[i]:key_up(key, mod)\n"
	"end\n"
"end\n"

"function Input.touch_began(touches)\n"
	"for i = __listener_count, 1, -1 do\n"
		"__listeners[i]:touch_began(touches)\n"
	"end\n"
"end\n"

"function Input.touch_canceled()\n"
	"for i = __listener_count, 1, -1 do\n"
		"__listeners[i]:touch_canceled()\n"
	"end\n"
"end\n"

"function Input.touch_ended(touches)\n"
	"for i = __listener_count, 1, -1 do\n"
		"__listeners[i]:touch_ended(touches)\n"
	"end\n"
"end\n"

"function Input.touch_moved(touches)\n"
	"for i = __listener_count, 1, -1 do\n"
		"__listeners[i]:touch_moved(touches)\n"
	"end\n"
"end\n"

"__modules[1] = function()\n"
	"local events = Input.__events\n"
	"for i = 1, events[0] do\n"
		"local e = events[i]\n"
		"Input[e[1]](e[2])\n"
	"end\n"
"end\n";

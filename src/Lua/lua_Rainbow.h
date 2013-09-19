const char Rainbow_lua[] =

/* Rainbow bootstrap */

"__rainbow_modules = { nil }\n"

"function __update(dt)\n"
	"local modules = __rainbow_modules\n"
	"for i = 1, #modules do\n"
		"modules[i](dt)\n"
	"end\n"
	"update(dt)\n"
"end\n"

/* rainbow.input */

"local Input = rainbow.input\n"

"local __count      = 0\n"
"local __listeners  = {}\n"

"function Input.subscribe(obj)\n"
	"__count = __count + 1\n"
	"__listeners[__count] = obj\n"
"end\n"

"function Input.unsubscribe(obj)\n"
	"for i = __count, 1, -1 do\n"
		"if __listeners[i] == obj then\n"
			"__listeners[i] = __listeners[__count]\n"
			"__listeners[__count] = nil\n"
			"__count = __count - 1\n"
			"break\n"
		"end\n"
	"end\n"
"end\n"

"function Input.unsubscribe_all()\n"
	"for i = 1, __count do\n"
		"__listeners[i] = nil\n"
	"end\n"
	"__count = 0\n"
"end\n"

"function Input.key_down(keys)\n"
	"local key, mod = keys[1], keys[2]\n"
	"for i = __count, 1, -1 do\n"
		"__listeners[i]:key_down(key, mod)\n"
	"end\n"
"end\n"

"function Input.key_up(keys)\n"
	"local key, mod = keys[1], keys[2]\n"
	"for i = __count, 1, -1 do\n"
		"__listeners[i]:key_up(key, mod)\n"
	"end\n"
"end\n"

"function Input.touch_began(touches)\n"
	"for i = __count, 1, -1 do\n"
		"__listeners[i]:touch_began(touches)\n"
	"end\n"
"end\n"

"function Input.touch_canceled()\n"
	"for i = __count, 1, -1 do\n"
		"__listeners[i]:touch_canceled()\n"
	"end\n"
"end\n"

"function Input.touch_ended(touches)\n"
	"for i = __count, 1, -1 do\n"
		"__listeners[i]:touch_ended(touches)\n"
	"end\n"
"end\n"

"function Input.touch_moved(touches)\n"
	"for i = __count, 1, -1 do\n"
		"__listeners[i]:touch_moved(touches)\n"
	"end\n"
"end\n"

"__rainbow_modules[1] = function()\n"
	"local events = Input.__events\n"
	"for i = 1, events[0] do\n"
		"local e = events[i]\n"
		"Input[e[1]](e[2])\n"
	"end\n"
"end\n";

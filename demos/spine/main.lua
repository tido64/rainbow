-- Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local boy

function create_boy()
    local boy = rainbow.skeleton("data/spineboy.json", 0.4)
    boy:set_animation_mix("idle", "run", 0.2)
    boy:set_animation_mix("walk", "run", 0.2)
    boy:set_animation_mix("run", "jump", 0.2)
    boy:set_animation_mix("jump", "run", 0.2)
    boy:set_animation_mix("run", "walk", 0.2)
    boy:set_animation_mix("walk", "idle", 0.2)
    boy:set_animation(0, "idle", true)
    return boy
end

function noop() end

function on_click()
    boy:set_animation(0, "run", true)
    boy:add_animation(0, "jump", false, 0.0)
    boy:add_animation(0, "run", true, 0.0)
    boy:add_animation(0, "walk", true, 0.0)
    boy:add_animation(0, "idle", true, 1.0)
end

function init()
    local rainbow = rainbow
    local screen = rainbow.platform.screen

    boy = create_boy()
    boy:set_position(screen.width * 0.33, screen.height * 0.1)
    rainbow.renderqueue:add(boy)

    local animation_state_handler = {
        on_animation_start = function(self, track, loop_count)
            print("> An animation has started on track: " .. track)
            print("  Number of times looped: " .. loop_count)
        end,
        on_animation_end = function(self, track, loop_count)
            print("> An animation has ended on track: " .. track)
            print("  Number of times looped: " .. loop_count)
        end,
        on_animation_complete = function(self, track, loop_count)
            print("> An animation has completed on track: " .. track)
            print("  Number of times looped: " .. loop_count)
        end,
        on_animation_event = function(self, track, event, loop_count)
            print("> An animation event has occurred on track: " .. track)
            print("  Name of the event: " ..  event)
            print("  Number of times looped: " .. loop_count)
        end
    }
    boy:set_listener(animation_state_handler)

    local input_delegate = {
        key_down = noop,
        key_up = noop,
        pointer_began = on_click,
        pointer_canceled = noop,
        pointer_ended = noop,
        pointer_moved = noop,
    }
    rainbow.input.subscribe(input_delegate)
end

function update(dt)
end

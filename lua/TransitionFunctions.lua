-- Transition easing functions.
--
-- Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local function cubic(t)
    return t * t * t
end

local function ease(a, b, t)
    return a + (b - a) * t
end

local function easeinout(f, a, b, t)
    return t < 0.5 and ease(a, b, f(t * 2.0) * 0.5)
                    or ease(a, b, 1 - f((1 - t) * 2.0) * 0.5)
end

local function flip(f, t)
    return 1.0 - f(1.0 - t)
end

local function quadratic(t)
    return t * t
end

local function quartic(t)
    return t * t * t * t
end

local function quintic(t)
    return t * t * t * t * t
end

return {
    linear = ease,

    easein_cubic = function(a, b, t)
        return ease(a, b, cubic(t))
    end,

    easein_quadratic = function(a, b, t)
        return ease(a, b, quadratic(t))
    end,

    easein_quartic = function(a, b, t)
        return ease(a, b, quartic(t))
    end,

    easein_quintic = function(a, b, t)
        return ease(a, b, quintic(t))
    end,

    easeout_cubic = function(a, b, t)
        return ease(a, b, flip(cubic, t))
    end,

    easeout_quadratic = function(a, b, t)
        return ease(a, b, flip(quadratic, t))
    end,

    easeout_quartic = function(a, b, t)
        return ease(a, b, flip(quartic, t))
    end,

    easeout_quintic = function(a, b, t)
        return ease(a, b, flip(quintic, t))
    end,

    easeinout_cubic = function(a, b, t)
        return easeinout(cubic, a, b, t)
    end,

    easeinout_quadratic = function(a, b, t)
        return easeinout(quadratic, a, b, t)
    end,

    easeinout_quartic = function(a, b, t)
        return easeinout(quartic, a, b, t)
    end,

    easeinout_quintic = function(a, b, t)
        return easeinout(quintic, a, b, t)
    end,
}

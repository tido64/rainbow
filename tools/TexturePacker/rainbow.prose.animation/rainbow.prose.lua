-- Created with TexturePacker (http://www.codeandweb.com/texturepacker)
--
-- {{smartUpdateKey}}
--
-- local Prose = require("Prose")
-- local {{texture.trimmedName}} = require("{{texture.trimmedName}}.prose")
-- local scene = Prose.from_table({{texture.trimmedName}})
{% load assembler %}
return {
	version = 1.1,
	resources = {
		{{texture.trimmedName}} = {
			"{{texture.fullName}}",{% for sprite in allSprites %}
			{ {{sprite.frameRect.x}}, {{sprite.frameRect.y}}, {{sprite.frameRect.width}}, {{sprite.frameRect.height}} },  -- {{forloop.counter0}} = {{sprite.trimmedName}}{% endfor %}
		}
	},
	nodes = {
		{
			name = "{{texture.trimmedName}}_batch",
			texture = "{{texture.trimmedName}}",
			sprites = {
				{
					name = "{{texture.trimmedName}}",
					size = { {{sprites|spriteSize}} },
					animations = {
						{{allSprites|animationsFilter}}
					},
					position = { 0, 0 },
					texture = 0
				}
			}
		}
	}
}

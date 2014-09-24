-- Created with TexturePacker (http://www.codeandweb.com/texturepacker)
--
-- {{smartUpdateKey}}
--
-- local Prose = require("Prose")
-- local {{texture.trimmedName}} = require("{{texture.trimmedName}}.prose")
-- local scene = Prose.from_table({{texture.trimmedName}})

return {
	version = 1.1,
	resources = {
		{{texture.trimmedName}} = {
			"{{texture.fullName}}",{% for sprite in sprites %}
			{ {{sprite.frameRect.x}}, {{sprite.frameRect.y}}, {{sprite.frameRect.width}}, {{sprite.frameRect.height}} },  -- {{forloop.counter0}} = {{sprite.trimmedName}}{% endfor %}
		}
	},
	nodes = {
		{
			name = "{{texture.trimmedName}}_batch",
			texture = "{{texture.trimmedName}}",
			sprites = {{% for sprite in allSprites %}
				{
					name = "{{sprite.trimmedName}}",
					size = { {{sprite.frameRect.width}}, {{sprite.frameRect.height}} },
					position = { 0, 0 },{% if sprite.rotated %}
					rotation = math.pi * 0.5,{% endif %}
					texture = {{forloop.counter0}}
				},{% endfor %}
			}
		}
	}
}

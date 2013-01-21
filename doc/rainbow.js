var header = {};
var functions = {};
var properties = {};
var tablesofcontent = {};

function generatefunctions(funcs, module) {
	var content = ''
	var toc = '';
	for (f in funcs) {
		var func = funcs[f];
		var entry;
		var name;
		if (func.name == '__call') {
			entry = module;
			name = entry + '(';
		}
		else if (func.name.charAt(0) == '.') {
			entry = module + func.name;
			name = entry + '(';
		}
		else if (func.name.charAt(0) == ':') {
			entry = module + func.name;
			name = '&lt;' + module + '&gt;' + func.name + '(';
		}
		else {
			console.log("[ERR] Invalid function name");
			continue;
		}
		var optional = false;
		var param = false;
		var params = '';
		for (p in func.parameters) {
			param = !param;
			if (!param) {
				if (optional) {
					params += '<td class="pdesc optional">';
					optional = false;
				}
				else {
					params += '<td class="pdesc">';
				}
				params += (typeof func.parameters[p] === 'undefined' ? 'TODO' : func.parameters[p]) + '</td></tr>';
			}
			else {
				params += '<tr><td class="mono param">';
				if (func.parameters[p].charAt(0) == '+') {
					params += func.parameters[p].slice(1);
					optional = true;
				}
				else {
					params += func.parameters[p];
				}
				params += '</td>';  // close td.param only, tr is closed with td.pdesc
				name += func.parameters[p] + ', ';
			}
		}
		if (params.length > 0) {
			name = name.slice(0, -2);
		}
		name += ')';
		var anchor = module + func.name;
		toc += '<li><a href="javascript:scrollTo(\'' + anchor + '\');">' + entry + '</a></li>';
		content += '<h3 id="' + anchor + '">' + name + '</h3>'
		         + '<div class="details">';
		if (params.length > 0) {
			content += '<h4>Parameters</h4>'
			         + '<table class="parameters">' + params + '</table>';
		}
		content += (typeof func.description === 'undefined' ? '' : func.description)
		         + '</div>';  // close div.details
	}
	tablesofcontent[module] += toc;
	functions[module] = content;
}

function generatelibraries(libs) {
	var list = '<ul>';
	for (i in libs)
		list += '<li><a href="' + libs[i].url + '">' + libs[i].name + '</a></li>';
	return list + '</ul>';
}

function generatemodule(module, data) {
	header[module] = '<h2>' + module + '</h2>'
	if (data.hasOwnProperty('description')) {
		header[module] += '<div class="description">' + data.description + '</div>'
	}
	tablesofcontent[module] = '';
	properties[module] = '';
	functions[module] = '';
}

function generateproperties(props, module) {
	var isprop = false;
	var prop;
	var content = '';
	var toc = '';
	for (p in props) {
		isprop = !isprop;
		if (isprop) {
			prop = props[p];
		}
		else {
			var anchor = module + '.' + prop;
			toc += '<li><a href="javascript:scrollTo(\'' + anchor + '\');">' + anchor + '</a></li>';
			content += '<h3 id="' + anchor + '">' + anchor + '</h3>';
			if (typeof props[p] === 'object') {
				content += '<table>';
				var odd = false;
				for (v in props[p]) {
					odd = !odd;
					if (odd) {
						content += '<tr><td class="mono">' + props[p][v] + '</td>';
					}
					else {
						content += '<td>' + props[p][v] + '</td></tr>';
					}
				}
				content += '</table>';
			}
			else {
				content += '<div class="details">'
				         + (typeof props[p] === 'undefined' ? '' : props[p])
				         + '</div>';
			}
		}
	}
	tablesofcontent[module] = toc;
	properties[module] = content;
}

function generate(modules, level) {
	var overview = '<ul>';
	for (field in modules) {
		var module = level + '.' + modules[field].name;
		overview += '<li><a href="#' + module + '">' + modules[field].name + "</a></li>";
		generatemodule(module, modules[field]);
		if (typeof modules[field].properties !== 'undefined') {
			generateproperties(modules[field].properties, module);
		}
		if (typeof modules[field].functions !== 'undefined') {
			generatefunctions(modules[field].functions, module);
		}
	}
	return overview + '</ul>';
}

function scrollTo(id) {
	document.getElementById(id).scrollIntoView();
}

function show(module) {
	if (module.length == 0) {
		document.getElementById('content').innerHTML
				= '<object data="gfx/angry-y-u-no.svg" type="image/svg+xml"></object>';
		return;
	}
	if (module.charAt(0) == '#') {
		module = module.slice(1);
	}
	document.getElementById('content').innerHTML
			= header[module]
			+ '<div class="toc">'
			+ '<h4>Table of content <a href="javascript:toggle(\'toc\')">+</a></h4>'
			+ '<ul id="toc">' + tablesofcontent[module] + '</ul>'
			+ '</div>'
			+ properties[module]
			+ functions[module];
}

function toggle(id) {
	var e = document.getElementById(id);
	e.style.display = (e.style.display == 'none') ? 'block' : 'none';
}

document.addEventListener('DOMContentLoaded', function() {
	document.getElementById('overview').innerHTML
			= '<div class="box">'
			+ '<h3>Rainbow</h3>'
			+ generate(rainbow.modules, 'rainbow')
			+ '</div>'
			+ '<div class="box">'
			+ '<h3>Libraries</h3>'
			+ generatelibraries(rainbow.libraries)
			+ '</div>';
	window.onhashchange = function() {
		show(window.location.hash);
		window.scroll(0, 0);
	};
	show(window.location.hash);
});

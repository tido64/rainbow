var createStringFromFrames = function(frames) {
  var str = frames.join(', ').replace(/^[\s,]+/, '').replace(/(?:\s,)*/g, '');
  return '{ ' + str + ', fps = ' + str.split(',').length + ' }';
};

var extractAnimationNameAndFrame = function(sprite) {
  return sprite.trimmedName.match(/(\D+)(\d+)/);
};

var sanitiseName = function(name) {
  return name.match(/(.*?)[^\dA-Za-z]*$/)[1];
};

var AnimationsFilter = function(sprites) {
  var animations = [];
  for (var i = 0; i < sprites.length; ++i) {
    var a = extractAnimationNameAndFrame(sprites[i]);
    if (!a)
      continue;
    var name = sanitiseName(a[1]);
    if (!animations[name])
      animations[name] = [];
    animations[name][parseInt(a[2])] = i;
  };
  var str = '';
  for (var name in animations)
    str += name + ' = ' + createStringFromFrames(animations[name]) + ',\n\t\t\t\t\t\t';
  return str.trim();
};
AnimationsFilter.filterName = "animationsFilter";
Library.addFilter("AnimationsFilter");

var SpriteSizeFilter = function(sprites) {
  var width = 0;
  var height = 0;
  for (var i = 0; i < sprites.length; ++i) {
    var frame = sprites[i].frameRect;
    if (frame.width > width)
      width = frame.width
    if (frame.height > height)
      height = frame.height;
  }
  return width + ', ' + height;
};
SpriteSizeFilter.filterName = "spriteSize";
Library.addFilter("SpriteSizeFilter");

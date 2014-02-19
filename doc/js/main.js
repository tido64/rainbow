(function() {
  var renderer = new marked.Renderer();
  renderer.table = function(header, body) {
      return '<table class="table table-bordered table-striped">' + body + '</table>';
    };
  var _ref = document.getElementById("content");
  _ref.innerHTML = marked(_ref.innerHTML, { renderer: renderer });
  hljs.initHighlightingOnLoad();

}).call(this);

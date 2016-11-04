var wso;

$(function () {
  $(document).keypress(consoleKeypress);
  var host = window.location.hostname;
  var port = window.location.port;

  wso = new WebSocket('ws:' + host + ':' + port + '/ws', 'shell');

  wso.onopen = function (e) {
    console.log(e);
  }
});

function consoleKeypress(e) {
  console.log(e);
  $('.cursor').before(e.key);
}

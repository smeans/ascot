var wso;

$(function () {
  $(document).keypress(consoleKeypress);
  var host = window.location.hostname;
  var port = window.location.port;

  wso = new WebSocket('ws:' + host + ':' + port + '/ws', 'shell');

  wso.onopen = function (e) {
    console.log('open:' + e);
  }

  wso.onerror = function (e) {
    console.log('error:' + e);
  }

  wso.onmessage = function (e) {
    $('.cursor').before(e.data);
  }

  wso.onclose = function (e) {
    console.log('open:' + e);
  }
});

function consoleKeypress(e) {
  if (wso.readyState == wso.OPEN) {
    wso.send(e.key);
  }
}

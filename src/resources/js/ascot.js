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
    console.log('->' + e.data);
    var m = e.data.replace('\n', '<br/>');

    $('.cursor').before(m);
  }

  wso.onclose = function (e) {
    console.log('open:' + e);
  }
});

function consoleKeypress(e) {
  if (wso.readyState == wso.OPEN) {
    var s;
    switch (e.key) {
      case 'Enter': {
        s = '\n';
      } break;
      default: {
        s = e.key;
      } break;
    }
    console.log('<-' + s);
    wso.send(s);
  }
}

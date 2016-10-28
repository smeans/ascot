$(function () {
  $(document).keypress(consoleKeypress);
});

function consoleKeypress(e) {
  console.log(e);
  $('.cursor').before(e.key);
}

const canvas = document.querySelector("#canvas")
canvas.addEventListener('contextmenu', (event) => {
  event.preventDefault();
});

window.Module = { canvas };

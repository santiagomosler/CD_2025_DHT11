import('./server.js')
  .then(() => {
    console.log('Backend iniciado correctamente desde main.js');
  })
  .catch((err) => {
    console.error('Error al iniciar el backend:', err);
  });

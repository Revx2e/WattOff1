const C='wattoff-v1';
const FILES=['./','./index.html','./manifest.json','./icon-192.png','./icon-512.png'];
self.addEventListener('install',e=>{e.waitUntil(caches.open(C).then(c=>c.addAll(FILES)));self.skipWaiting();});
self.addEventListener('activate',e=>e.waitUntil(self.clients.claim()));
self.addEventListener('fetch',e=>{e.respondWith(caches.match(e.request).then(r=>r||fetch(e.request).then(res=>{
  if(e.request.method==='GET'&&res.ok){const cp=res.clone();caches.open(C).then(c=>c.put(e.request,cp));}return res;})));});
self.addEventListener('notificationclick',e=>{e.notification.close();e.waitUntil(clients.openWindow('./index.html'));});

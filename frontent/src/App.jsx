import React, { useState, useEffect } from 'react';
import { Car, Zap, Activity, Clock, Shield, Map, Monitor, Database, Radio, X, Octagon, RotateCcw } from 'lucide-react';

export default function App() {
  const [data, setData] = useState(null);
  const [booting, setBooting] = useState(true);
  const [bootProgress, setBootProgress] = useState(0);
  const [vehicleId, setVehicleId] = useState('');
  const [zoneId, setZoneId] = useState('1');
  const [statusMsg, setStatusMsg] = useState('');

  // Simulation of System Boot
  useEffect(() => {
    const timer = setInterval(() => {
      setBootProgress(prev => {
        if (prev >= 100) {
          clearInterval(timer);
          setTimeout(() => setBooting(false), 800);
          return 100;
        }
        return prev + Math.floor(Math.random() * 15);
      });
    }, 200);
    return () => clearInterval(timer);
  }, []);

  const fetchData = async () => {
    try {
      const res = await fetch('http://localhost:8080/api/data');
      if (!res.ok) throw new Error("Failed");
      setData(await res.json());
    } catch (e) { console.error(e); }
  };

  useEffect(() => {
    if (!booting) {
      fetchData();
      const interval = setInterval(fetchData, 1500);
      return () => clearInterval(interval);
    }
  }, [booting]);

  const handleRequest = async (e) => {
    e.preventDefault();
    if (!vehicleId) return;
    setStatusMsg('TRANSMITTING...');
    try {
      const res = await fetch('http://localhost:8080/api/request', {
        method: 'POST', body: `vehicleId=${vehicleId}&zoneId=${zoneId}`,
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
      });
      const json = await res.json();
      setStatusMsg(json.requestId ? `TARGET ACQUIRED: SLOT ASSIGNED` : 'ALLOCATION FAILED');
      setVehicleId('');
      fetchData();
      setTimeout(() => setStatusMsg(''), 3000);
    } catch { setStatusMsg('CONNECTION ERROR') }
  };

  const handleCancel = async (id) => {
    await fetch('http://localhost:8080/api/cancel', {
      method: 'POST', body: `requestId=${id}`,
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
    });
    fetchData();
  };

  const handleLeave = async (id) => {
    await fetch('http://localhost:8080/api/leave', {
      method: 'POST', body: `requestId=${id}`,
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
    });
    fetchData();
  };

  const handleRollback = async () => {
    await fetch('http://localhost:8080/api/rollback', { method: 'POST', body: 'k=1' });
    setStatusMsg('TEMPORAL REVERSION COMPLETE');
    fetchData();
    setTimeout(() => setStatusMsg(''), 3000);
  };

  // --- BOOT LOADER ---
  if (booting) {
    return (
      <div className="h-screen bg-black flex flex-col items-center justify-center relative overflow-hidden">
        {/* Abstract Background Grid */}
        <div className="absolute inset-0 bg-[linear-gradient(rgba(0,20,0,0.2)_1px,transparent_1px),linear-gradient(90deg,rgba(0,20,0,0.2)_1px,transparent_1px)] bg-[size:40px_40px] opacity-20"></div>

        <div className="w-64 h-64 relative flex items-center justify-center loader-container">
          <div className="absolute inset-0 scanner-ring"></div>
          <div className="absolute inset-4 scanner-ring border-t-cyan-500/50" style={{ animationDirection: 'reverse', animationDuration: '2s' }}></div>
          <div className="w-20 h-20 rounded-full scanner-core blur-md"></div>
          <Car size={32} className="text-cyan-400 relative z-10 animate-pulse" />
        </div>

        <div className="mt-8 font-display text-cyan-500 text-2xl tracking-[0.2em] font-bold">
          SYSTEM BOOT
        </div>
        <div className="w-64 h-1 bg-slate-800 mt-4 rounded-full overflow-hidden neon-border">
          <div className="h-full bg-cyan-500 transition-all duration-200" style={{ width: `${bootProgress}%` }}></div>
        </div>
        <div className="mt-2 text-cyan-500/50 font-mono text-xs">
          {bootProgress < 100 ? `LOADING MODULES... ${bootProgress}%` : 'INITIALIZATION COMPLETE'}
        </div>
      </div>
    );
  }

  // --- MAIN DASHBOARD ---
  if (!data) return <div className="h-screen bg-black text-red-500 font-mono flex items-center justify-center text-xl">FATAL ERROR: BACKEND OFFLINE</div>;

  return (
    <div className="min-h-screen bg-slate-950 text-slate-200 font-sans selection:bg-cyan-500/30 overflow-hidden relative">
      <div className="scanline"></div>

      {/* Top HUB */}
      <header className="fixed top-0 w-full z-50 cyber-glass border-b border-cyan-500/10 h-16 flex items-center justify-between px-8">
        <div className="flex items-center gap-4">
          <div className="p-2 bg-cyan-500/10 rounded border border-cyan-500/30">
            <Monitor size={20} className="text-cyan-400" />
          </div>
          <div>
            <h1 className="text-xl font-bold text-white tracking-widest neon-text-blue">ZONENET <span className="text-cyan-500">ALPHA</span></h1>
            <div className="flex items-center gap-2 text-[10px] text-cyan-400/60 font-mono">
              <span className="w-1.5 h-1.5 bg-emerald-500 rounded-full animate-pulse"></span>
              LIVE UPLINK ACTIVE
            </div>
          </div>
        </div>

        <div className="flex items-center gap-6">
          <div className="flex flex-col items-end">
            <span className="text-[10px] text-slate-500 font-bold tracking-widest">SYSTEM TIME</span>
            <span className="font-mono text-cyan-300"><Clock size={12} className="inline mr-1" />{new Date().toLocaleTimeString()}</span>
          </div>
          <button onClick={handleRollback} className="group relative px-4 py-2 bg-rose-900/10 border border-rose-500/30 hover:border-rose-500 hover:bg-rose-900/20 text-rose-400 text-xs font-bold rounded overflow-hidden transition-all">
            <span className="relative z-10 flex items-center gap-2"><RotateCcw size={14} /> UNDO SEQUENCE</span>
            <div className="absolute inset-0 bg-rose-500/10 translate-y-full group-hover:translate-y-0 transition-transform"></div>
          </button>
        </div>
      </header>

      {/* Main Content */}
      <div className="pt-24 pb-8 px-8 grid grid-cols-12 gap-8 h-screen">

        {/* Left Column: Command & Logs */}
        <div className="col-span-12 lg:col-span-4 flex flex-col gap-6 h-full overflow-hidden pb-8">

          {/* Command Input */}
          <div className="cyber-glass rounded-xl p-6 border-l-4 border-l-cyan-500 relative">
            <div className="absolute top-2 right-2 text-cyan-500/20"><Radio size={40} /></div>
            <h2 className="text-sm font-bold text-cyan-400 mb-6 flex items-center gap-2 tracking-widest">
              <Zap size={16} /> DATA ENTRY
            </h2>

            <form onSubmit={handleRequest} className="space-y-4 relative z-10">
              <div>
                <label className="block text-[10px] font-bold text-slate-500 mb-1 tracking-wider">VEHICLE IDENTIFIER</label>
                <input
                  value={vehicleId}
                  onChange={e => setVehicleId(e.target.value.toUpperCase())}
                  className="w-full bg-slate-900/80 border border-slate-700/50 rounded p-3 text-cyan-300 font-mono focus:border-cyan-500 focus:shadow-[0_0_10px_rgba(6,182,212,0.2)] outline-none transition"
                  placeholder="ENTER ID..."
                  autoFocus
                />
              </div>
              <div>
                <label className="block text-[10px] font-bold text-slate-500 mb-1 tracking-wider">TARGET SECTOR</label>
                <div className="flex gap-2">
                  {data.zones.map(z => (
                    <button
                      key={z.id}
                      type="button"
                      onClick={() => setZoneId(String(z.id))}
                      className={`flex-1 py-3 rounded border font-bold text-sm transition-all ${zoneId === String(z.id)
                        ? 'bg-cyan-500/20 border-cyan-500 text-cyan-300 shadow-[0_0_10px_rgba(6,182,212,0.2)]'
                        : 'bg-slate-900/50 border-slate-700 text-slate-500 hover:border-slate-500 hover:text-slate-300'
                        }`}
                    >
                      SEC-{z.id}
                    </button>
                  ))}
                </div>
              </div>
              <button className="w-full py-4 bg-gradient-to-r from-cyan-600 to-blue-600 text-white font-bold tracking-widest rounded shadow-lg hover:shadow-cyan-500/20 hover:scale-[1.02] active:scale-[0.98] transition-all flex items-center justify-center gap-2">
                <Monitor size={16} /> INITIATE ALLOCATION
              </button>
              {statusMsg && <div className="text-center text-xs font-mono text-cyan-400 animate-pulse">{statusMsg}</div>}
            </form>
          </div>

          {/* Logs */}
          <div className="flex-1 cyber-glass rounded-xl p-1 flex flex-col min-h-0">
            <div className="flex-1 cyber-glass rounded-xl p-4 flex flex-col relative overflow-hidden min-h-0">
              <div className="flex justify-between items-center mb-4 pb-2 border-b border-white/5">
                <h2 className="text-sm font-bold text-purple-400 tracking-widest flex items-center gap-2"><Database size={14} /> SYSTEM LOGS</h2>
              </div>

              {/* Real-time Analytics Summary */}
              <div className="grid grid-cols-2 gap-2 mb-4">
                <div className="bg-slate-900/50 p-2 rounded border border-slate-700/50">
                  <div className="text-[8px] text-slate-500 uppercase tracking-widest">Avg Duration</div>
                  <div className="text-lg font-mono text-cyan-400">
                    {data.requests.filter(r => r.state === 'RELEASED').length > 0
                      ? (data.requests.filter(r => r.state === 'RELEASED').reduce((acc, r) => acc + r.duration, 0) / data.requests.filter(r => r.state === 'RELEASED').length).toFixed(1)
                      : '0.0'}s
                  </div>
                </div>
                <div className="bg-slate-900/50 p-2 rounded border border-slate-700/50">
                  <div className="text-[8px] text-slate-500 uppercase tracking-widest">Completion</div>
                  <div className="text-lg font-mono text-emerald-400">
                    {data.requests.filter(r => r.state === 'RELEASED').length} / {data.requests.length}
                  </div>
                </div>
              </div>

              <div className="flex-1 overflow-y-auto space-y-2 custom-scrollbar pr-2">
                {data.requests.slice().reverse().map(req => (
                  <div key={req.id} className="bg-slate-900/60 p-3 rounded border-l-2 border-l-slate-700 hover:border-l-cyan-500 transition-all flex justify-between items-center group">
                    <div>
                      <div className="font-mono text-xs text-slate-500">OP-{req.id.toString().padStart(4, '0')}</div>
                      <div className="font-bold text-slate-300 text-sm">{req.vehicleId}</div>
                    </div>
                    <div className="text-right flex flex-col items-end gap-1">
                      <div className={`text-[10px] font-bold px-2 py-0.5 rounded ${req.state === 'ALLOCATED' ? 'bg-emerald-500/20 text-emerald-400' :
                        req.state === 'CANCELLED' ? 'bg-rose-500/20 text-rose-400' :
                          req.state === 'RELEASED' ? 'bg-blue-500/20 text-blue-400' : 'bg-slate-800 text-slate-400'
                        }`}>{req.state}</div>

                      {req.state === 'ALLOCATED' && (
                        <div className="flex gap-1 opacity-0 group-hover:opacity-100 transition-opacity">
                          <button
                            onClick={() => handleLeave(req.id)}
                            className="text-[9px] bg-blue-500/10 hover:bg-blue-500/30 text-blue-300 px-2 py-0.5 rounded border border-blue-500/30 uppercase tracking-wider"
                          >
                            DEPART
                          </button>
                          <button
                            onClick={() => handleCancel(req.id)}
                            className="text-[9px] bg-rose-500/10 hover:bg-rose-500/30 text-rose-400 px-2 py-0.5 rounded border border-rose-500/30 uppercase tracking-wider"
                          >
                            CANCEL
                          </button>
                        </div>
                      )}
                      {req.state === 'RELEASED' && <div className="text-[9px] text-slate-500 font-mono">{req.duration}s</div>}
                    </div>
                  </div>
                ))}
              </div>
            </div>
          </div>

        </div>

        {/* Right Column: Visualizer */}
        <div className="col-span-12 lg:col-span-8 flex flex-col h-full overflow-hidden pb-8">
          <div className="cyber-glass rounded-xl p-1 h-full flex flex-col relative overflow-hidden">
            {/* Map Header */}
            <div className="absolute top-0 left-0 right-0 p-6 z-10 flex justify-between items-start pointer-events-none">
              <div>
                <h2 className="text-2xl font-display font-bold text-white tracking-widest neon-text-blue">SECTOR MAP</h2>
                <div className="text-xs text-cyan-400/60 font-mono">ORTHOGRAPHIC PROJECTION // LIVE</div>
              </div>
              <div className="flex gap-4">
                <div className="flex flex-col items-center">
                  <span className="text-2xl font-bold text-white font-mono">{data.requests.filter(r => r.state === 'ALLOCATED').length}</span>
                  <div className="text-[8px] text-slate-500 uppercase tracking-widest">Active Units</div>
                </div>
              </div>
            </div>

            {/* Floor Grid - handled by sector-map-bg */}
            <div className="flex-1 sector-map-bg relative flex items-center justify-center perspective-container overflow-hidden">
              <div className="absolute inset-0 bg-gradient-to-t from-black via-transparent to-transparent z-0"></div>

              <div className="parking-grid-3d grid grid-cols-3 gap-12 mt-20 z-0">
                {data.zones.map(zone => (
                  <div key={zone.id} className="w-64 sci-fi-card rounded-xl p-4 transform hover:translate-z-10 transition-transform group">
                    {/* Tech Corners */}
                    <div className="tech-corner tl"></div>
                    <div className="tech-corner tr"></div>
                    <div className="tech-corner bl"></div>
                    <div className="tech-corner br"></div>

                    {/* Holographic floor effect */}
                    <div className="absolute top-full left-0 w-full h-10 bg-cyan-500/10 blur-xl transform scale-x-75 opacity-0 group-hover:opacity-100 transition-opacity"></div>

                    <div className="flex justify-between items-center mb-4 border-b border-cyan-500/20 pb-2 relative z-10">
                      <h3 className="text-lg font-bold text-cyan-300 flex items-center gap-2 font-display tracking-wider">
                        <Shield size={16} /> SEC-{zone.id}
                      </h3>
                      <div className="w-2 h-2 bg-emerald-400 rounded-full animate-pulse shadow-[0_0_10px_#34d399]"></div>
                    </div>

                    <div className="space-y-4 relative z-10">
                      {zone.areas.map(area => (
                        <div key={area.id} className="bg-black/60 rounded p-3 border border-slate-700/50 backdrop-blur-sm">
                          <div className="flex flex-wrap gap-4 justify-center">
                            {area.slots.map(slot => (
                              <div
                                key={slot.id}
                                className={`
                                                                w-12 h-16 border rounded flex items-center justify-center relative transition-all duration-300
                                                                ${slot.occupied
                                    ? 'bg-rose-500/10 border-rose-500/50 shadow-[0_0_10px_rgba(244,63,94,0.2)]'
                                    : 'bg-cyan-500/5 border-cyan-500/20 hover:border-cyan-400 hover:bg-cyan-500/10'
                                  }
                                                            `}
                              >
                                {slot.occupied ? (
                                  <div className="animate-bounce">
                                    <Car size={24} className="text-rose-400" />
                                  </div>
                                ) : (
                                  <span className="text-[10px] font-mono text-cyan-700">{slot.id}</span>
                                )}
                                <div className="absolute -bottom-1 w-full text-center text-[8px] font-bold text-slate-500">S-{slot.id}</div>
                              </div>
                            ))}
                          </div>
                        </div>
                      ))}
                    </div>

                    {/* Zone stats footer */}
                    <div className="mt-4 flex justify-between text-[10px] font-mono text-slate-500">
                      <span>CONN: {zone.id === 1 ? '2' : zone.id === 2 ? '1' : 'NULL'}</span>
                      <span>LOAD: {Math.round((zone.areas.reduce((a, ar) => a + ar.slots.filter(s => s.occupied).length, 0) / zone.areas.reduce((a, ar) => a + ar.slots.length, 0)) * 100)}%</span>
                    </div>
                  </div>
                ))}
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

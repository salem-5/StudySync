// src/app/page.tsx
import Hero from "@/components/Hero";
import Showcase from "@/components/Showcase";
import Features from "@/components/Features";
import Downloads from "@/components/Downloads";

export default function Home() {
  return (
      <main>
        <Hero />
        <Showcase />
        <Features />
        <Downloads />
      </main>
  );
}
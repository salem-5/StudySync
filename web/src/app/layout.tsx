import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";
import Navbar from "@/components/Navbar";
import Footer from "@/components/Footer";
import Script from "next/script";

const inter = Inter({ subsets: ["latin"] });

export const metadata: Metadata = {
  title: "StudySync | Academic Productivity",
  description: "A native C++ environment built for speed. Manage tasks, collaborate with groups, and focus with zero friction.",
};

export default function RootLayout({
                                     children,
                                   }: {
  children: React.ReactNode;
}) {
  return (
      <html lang="en">
      <head>
        <Script src="https://unpkg.com/@phosphor-icons/web" strategy="beforeInteractive" />
      </head>
      <body className={inter.className}>
      <div className="ambient-glow"></div>
      <Navbar />
      {children}
      <Footer />
      </body>
      </html>
  );
}